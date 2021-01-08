interface ContaFileFormat {
	populationSize: number
	communities: number
	ticks: HumanFormat[][]
}

interface HumanFormat {
	communityID: number
	positionX: number
	positionY: number
	infected: boolean
	recovered: boolean
}

let data: ContaFileFormat
let canvasses: HTMLCanvasElement[]
let ctxes: CanvasRenderingContext2D[]
let animationIntervalID: number

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

// Conta file format:
// "CONTA\n"
// [ int32 size ]
// [ int32 communities ]
// "\n"
// `number_of_ticks` times:
// `size` times:
// [ uint32 human ]
	// [ uint6 COMMUNITY_ID ]
	// [ uint9 POSITION_X ]
	// [ uint9 POSITION_Y ]
	// [ uint8 FLAGS ] ( 0 0 0 0 0 0 RECOVERED INFECTED )

const parseContaFile = (
	buffer: ArrayBuffer
): ContaFileFormat => {
	const contaFileFormat: ContaFileFormat = {
		populationSize: null,
		communities: null,
		ticks: []
	}

	const fileBuffer = new FileBuffer(buffer)

	// Expect conta file header

	if (fileBuffer.readString(6) != 'CONTA\n') {
		throw new Error(`Invalid file header`)
	}

	// Read the population size

	contaFileFormat.populationSize = fileBuffer.readInt32()

	// Read the number of communities

	contaFileFormat.communities = fileBuffer.readInt32()

	// Expect newline

	if (fileBuffer.readChar() != '\n') {
		throw new Error(`Expected newline after the file header, found another character`)
	}

	// Read all ticks

	while (fileBuffer.offset < fileBuffer.size()) {
		contaFileFormat.ticks.push(Array(contaFileFormat.populationSize))
		const humans = contaFileFormat.ticks[contaFileFormat.ticks.length - 1]

		for (let i = 0; i < contaFileFormat.populationSize; i++) {
			const human = fileBuffer.readUint32()

			// Forgive me for the below formatting please...
			// It's better than centre aligning my code ¯\_(ツ)_/¯

			const communityID = (human & 0b11111100000000000000000000000000) >> 26
			const positionX   = (human & 0b00000011111111100000000000000000) >> 17
			const positionY   = (human & 0b00000000000000011111111100000000) >> 8
			const flags       = (human & 0b00000000000000000000000111111111)

			const infected = Boolean(flags & (1 << 0))
			const recovered = Boolean(flags & (1 << 1))

			// JS, why are you doing this to me???

			const actualCommunityID = (communityID < 0) ? communityID + 64 : communityID

			humans[i] = { communityID: actualCommunityID, positionX, positionY, infected, recovered }
		}
	}

	return contaFileFormat
}

const renderTick = (
	tickNumber: number
) => {
	// Clear all ctxes

	for (let i = 0; i < ctxes.length; i++) {
		ctxes[i].clearRect(0, 0, canvasses[i].width, canvasses[i].height)
	}

	// Draw all humans

	const graphData: GraphData = {
		tick: tickNumber,
		infected: 0,
		recovered: 0,
		susceptible: 0
	}

	for (let i = 0; i < data.populationSize; i++) {
		const {
			communityID,
			positionX,
			positionY,
			infected,
			recovered
		} = data.ticks[tickNumber][i]

		if (infected) graphData.infected++
		else if (!recovered) graphData.susceptible++

		if (recovered) graphData.recovered++

		const ctx = ctxes[communityID]

		ctx.beginPath()
		ctx.arc(positionX, positionY, 3, 0, 2 * Math.PI)
		ctx.fillStyle = recovered ? '#3f3' : infected ? '#f33' : '#aaa'
		ctx.fill()
		ctx.closePath()
	}

	// Update progress bar

	const progressBar = document.querySelector<HTMLSpanElement>('.progress')
	progressBar.style.width = (tickNumber / data.ticks.length * 100).toString() + '%'

	// Update graph

	updateGraph(graphData)
}

const startAnimation = () => {
	// Reset the interval

	clearInterval(animationIntervalID)

	let i = 0
	const renderNextTick = () => renderTick(i++)

	// Render the first tick

	renderNextTick()

	// Render the next ticks

	animationIntervalID = window.setInterval(() => {
		renderNextTick()

		// Stop the animation when we rendered all ticks

		if (i == data.ticks.length) {
			clearInterval(animationIntervalID)
		}
	}, 16.7)
}

// When page loads, show the run

addEventListener('load', async () => {
	const searchParams = new URLSearchParams(location.search)
	const output = await getRunOutput(searchParams.get('id'))
	data = parseContaFile(output)

	// Create communities

	const canvassesContainer = document.querySelector<HTMLDivElement>('.communities')
	canvasses = Array(data.communities)
	ctxes = Array(data.communities)

	canvassesContainer.innerHTML = ''

	for (let i = 0; i < data.communities; i++) {
		// Create canvas

		const canvas = document.createElement('canvas')
		canvas.classList.add('community')
		canvas.width = 512
		canvas.height = 512

		// Add canvas

		canvassesContainer.appendChild(canvas)
		canvasses[i] = canvas
		ctxes[i] = canvas.getContext('2d')
	}

	// Create graph

	createGraph()

	console.log(data)
})