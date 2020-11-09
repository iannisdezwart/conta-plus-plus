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

class FileBuffer {
	buffer: ArrayBuffer
	array: Uint8Array
	offset = 0

	constructor(buffer: ArrayBuffer) {
		this.buffer = buffer
		this.array = new Uint8Array(buffer)
	}

	// Utility functions

	size() {
		return this.buffer.byteLength
	}

	ended() {
		return this.offset >= this.size()
	}

	// Reading functions

	readByte() {
		return this.array[this.offset++]
	}

	readChar() {
		const charCode = this.readByte()
		return String.fromCharCode(charCode)
	}

	readString(length: number) {
		let output = ''

		for (let i = 0; i < length; i++) {
			output += this.readChar()
		}

		return output
	}

	readUint8() {
		const asUint8 = this.readByte()

		return asUint8
	}

	readInt8() {
		const asInt8 = this.readByte()

		if (asInt8 & (1 << 7)) return ~asInt8
		return asInt8
	}

	readUint16() {
		const byte0 = this.readByte()
		const byte1 = this.readByte()
		const asUint16 = byte0 << 8 + byte1

		return asUint16
	}

	readInt16() {
		const byte0 = this.readByte()
		const byte1 = this.readByte()
		const asUint16 = byte0 << 8 + byte1

		if (asUint16 & (1 << 15)) return ~asUint16
		return asUint16
	}

	readUint32() {
		const byte0 = this.readByte()
		const byte1 = this.readByte()
		const byte2 = this.readByte()
		const byte3 = this.readByte()
		const asUint32 = byte0 << 24 + byte1 << 16 + byte2 << 8 + byte3

		return asUint32
	}

	readInt32() {
		const byte0 = this.readByte()
		const byte1 = this.readByte()
		const byte2 = this.readByte()
		const byte3 = this.readByte()
		const asUint32 = byte0 << 24 + byte1 << 16 + byte2 << 8 + byte3

		if (asUint32 & (1 << 31)) return ~asUint32
		return asUint32
	}
}

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
	// Todo: fix this infinite loop

	while (fileBuffer.offset < fileBuffer.size() - 1) {
		contaFileFormat.ticks.push(Array(contaFileFormat.size))
		const humans = contaFileFormat.ticks[contaFileFormat.ticks.length - 1]

		for (let i = 0; i < contaFileFormat.size; i++) {
			humans[i].positionX = fileBuffer.readUint16()
			humans[i].positionY = fileBuffer.readUint16()
		}
	}

	// Expect newline

	if (fileBuffer.readChar() != '\n') {
		throw new Error(`Expected newline at the end of the file, found another character`)
	}

	return contaFileFormat
}