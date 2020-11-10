// File format:
// "CONTA\n"
// [ int32 size ]
// [ int32 communities ]
// "\n"
// `number_of_ticks` times:
// `size` times:
// [ uint16 POSITION_X ]
// [ uint16 POSITION_Y ]

interface ContaFileFormat {
	size: number
	communities: number
	ticks: HumanFormat[][]
}

interface HumanFormat {
	positionX: number
	positionY: number
}

// This function gets a run output

const getRunOutput = (
	runID: string
) => new Promise<ArrayBuffer>((resolve, reject) =>{
	const req = new XMLHttpRequest()
	req.responseType = 'arraybuffer'

	req.open('POST', '/get-run-output')
	req.setRequestHeader('run-id', runID)

	req.addEventListener('load', () => {
		resolve(req.response as ArrayBuffer)
	})

	req.addEventListener('error', reject)

	req.send()
})

// This function creates a ContaFileFormat from a raw conta run output file

const parseContaFile = (
	buffer: ArrayBuffer
): ContaFileFormat => {
	const contaFileFormat: ContaFileFormat = {
		size: null,
		communities: null,
		ticks: []
	}

	const fileBuffer = new FileBuffer(buffer)

	// Expect conta file header

	if (fileBuffer.readString(6) != 'CONTA\n') {
		throw new Error(`Invalid file header`)
	}

	// Read the population size

	contaFileFormat.size = fileBuffer.readInt32()

	// Read the number of communities

	contaFileFormat.communities = fileBuffer.readInt32()

	// Expect newline

	if (fileBuffer.readChar() != '\n') {
		throw new Error(`Expected newline after the file header, found another character`)
	}

	// Read all ticks

	while (fileBuffer.offset < fileBuffer.size()) {
		contaFileFormat.ticks.push(Array(contaFileFormat.size))
		const humans = contaFileFormat.ticks[contaFileFormat.ticks.length - 1]

		for (let i = 0; i < contaFileFormat.size; i++) {
			humans[i] = {
				positionX: fileBuffer.readUint16(),
				positionY: fileBuffer.readUint16()
			}
		}
	}

	return contaFileFormat
}