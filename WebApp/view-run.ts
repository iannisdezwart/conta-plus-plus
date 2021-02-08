interface ContaFileFormatLowMem {
	populationSize: number
	communities: number
	ticks: Uint32Array[]
}

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
	incubating: boolean
}

let data: ContaFileFormat | ContaFileFormatLowMem
let canvasses: HTMLCanvasElement[]
let ctxes: CanvasRenderingContext2D[]
let animationIntervalID: number

const searchParams = new URLSearchParams(location.search)
const lowMemMode = searchParams.has('low-memory')

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
	// [ uint11 COMMUNITY_ID ]
	// [ uint9 POSITION_X ]
	// [ uint9 POSITION_Y ]
	// [ uint3 FLAGS ] ( INCUBAING RECOVERED INFECTED )

const readHuman = (human: number) => {
	// Forgive me for the below formatting please...
	// It's better than centre aligning my code ¯\_(ツ)_/¯

	const communityID = (human & 0b11111111111000000000000000000000) >> 21
	const positionX   = (human & 0b00000000000111111111000000000000) >> 12
	const positionY   = (human & 0b00000000000000000000111111111000) >> 3
	const flags       = (human & 0b00000000000000000000000000000111)

	const infected = Boolean(flags & (1 << 0))
	const recovered = Boolean(flags & (1 << 1))
	const incubating = Boolean(flags & (1 << 2))

	// JS, why are you doing this to me???

	const actualCommunityID = (communityID < 0) ? communityID + 2048 : communityID

	return {
		communityID: actualCommunityID,
		positionX, positionY,
		infected, recovered, incubating
	}
}

const parseContaFileLowMem = (
	buffer: ArrayBuffer
): ContaFileFormatLowMem => {
	const contaFileFormat: ContaFileFormatLowMem = {
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
		contaFileFormat.ticks.push(new Uint32Array(contaFileFormat.populationSize))
		const humans = contaFileFormat.ticks[contaFileFormat.ticks.length - 1]

		for (let i = 0; i < contaFileFormat.populationSize; i++) {
			humans[i] = fileBuffer.readUint32()
		}
	}

	return contaFileFormat
}

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

			const communityID = (human & 0b11111111111000000000000000000000) >> 21
			const positionX   = (human & 0b00000000000111111111000000000000) >> 12
			const positionY   = (human & 0b00000000000000000000111111111000) >> 3
			const flags       = (human & 0b00000000000000000000000000000111)

			const infected = Boolean(flags & (1 << 0))
			const recovered = Boolean(flags & (1 << 1))
			const incubating = Boolean(flags & (1 << 2))

			// JS, why are you doing this to me???

			const actualCommunityID = (communityID < 0) ? communityID + 2048 : communityID

			humans[i] = { communityID: actualCommunityID, positionX, positionY, infected, recovered, incubating }
		}
	}

	return contaFileFormat
}

const renderTickLowMem = (
	tickNumber: number
) => {
	const start = Date.now()

	// Gather Stats

	const graphData: GraphData = {
		tick: tickNumber,
		infected: 0,
		incubating: 0,
		susceptible: 0,
		recovered: 0
	}

	for (let i = 0; i < data.populationSize; i++) {
		data = data as ContaFileFormatLowMem

		const {
			infected, recovered, incubating
		} = readHuman(data.ticks[tickNumber][i])

		if (!recovered && !infected) graphData.susceptible++
		if (infected)
			if (incubating) graphData.incubating++
			else graphData.infected++
		if (recovered) graphData.recovered++
	}

	// Update progress bar

	const progressBar = document.querySelector<HTMLSpanElement>('.progress')
	progressBar.style.width = (tickNumber / data.ticks.length * 100).toString() + '%'

	// Update graph

	updateGraph(graphData)

	// Update current data display

	const infectedCountEl = document.querySelector<HTMLSpanElement>('span#infected')
	const incubatingCountEl = document.querySelector<HTMLSpanElement>('span#incubating')
	const susceptibleCountEl = document.querySelector<HTMLSpanElement>('span#susceptible')
	const recoveredCountEl = document.querySelector<HTMLSpanElement>('span#recovered')

	infectedCountEl.innerText = graphData.infected.toString()
	incubatingCountEl.innerText = graphData.incubating.toString()
	susceptibleCountEl.innerText = graphData.susceptible.toString()
	recoveredCountEl.innerText = graphData.recovered.toString()

	const duration = Date.now() - start

	if (tickNumber + 1 != data.ticks.length) {
		// Queue next tick

		animationIntervalID = setTimeout(() => renderTickLowMem(tickNumber + 1), 0)
	}

	if (tickNumber % 3 == 0) {
		// Show FPS

		const fpsEl = document.querySelector<HTMLSpanElement>('span#fps')
		fpsEl.innerText = (1000 / duration).toFixed(1)
	}
}

const renderTick = (
	tickNumber: number
) => {
	const start = Date.now()

	// Clear all ctxes

	for (let i = 0; i < ctxes.length; i++) {
		ctxes[i].clearRect(0, 0, canvasses[i].width, canvasses[i].height)
	}

	// Draw all humans

	const graphData: GraphData = {
		tick: tickNumber,
		infected: 0,
		incubating: 0,
		susceptible: 0,
		recovered: 0
	}

	for (let i = 0; i < data.populationSize; i++) {
		data = data as ContaFileFormat

		const {
			communityID,
			positionX,
			positionY,
			infected,
			recovered,
			incubating
		} = data.ticks[tickNumber][i]

		if (!recovered && !infected) graphData.susceptible++
		if (infected)
			if (incubating) graphData.incubating++
			else graphData.infected++
		if (recovered) graphData.recovered++

		const ctx = ctxes[communityID]

		ctx.beginPath()
		ctx.arc(positionX, positionY, 3, 0, 2 * Math.PI)

		if (recovered) ctx.fillStyle = '#3f3'
		else if (infected)
			if (incubating) ctx.fillStyle = '#fb3'
			else ctx.fillStyle = '#f33'
		else ctx.fillStyle = '#aaa'

		ctx.fill()
		ctx.closePath()
	}

	// Update progress bar

	const progressBar = document.querySelector<HTMLSpanElement>('.progress')
	progressBar.style.width = (tickNumber / data.ticks.length * 100).toString() + '%'

	// Update graph

	updateGraph(graphData)

	// Update current data display

	const infectedCountEl = document.querySelector<HTMLSpanElement>('span#infected')
	const incubatingCountEl = document.querySelector<HTMLSpanElement>('span#incubating')
	const susceptibleCountEl = document.querySelector<HTMLSpanElement>('span#susceptible')
	const recoveredCountEl = document.querySelector<HTMLSpanElement>('span#recovered')

	infectedCountEl.innerText = graphData.infected.toString()
	incubatingCountEl.innerText = graphData.incubating.toString()
	susceptibleCountEl.innerText = graphData.susceptible.toString()
	recoveredCountEl.innerText = graphData.recovered.toString()

	const duration = Date.now() - start
	const timeout = Math.max(0, 16.7 - duration)

	if (tickNumber + 1 != data.ticks.length) {
		// Queue next tick

		animationIntervalID = setTimeout(() => renderTick(tickNumber + 1), timeout)
	}

	if (tickNumber % 3 == 0) {
		// Show FPS

		const fpsEl = document.querySelector<HTMLSpanElement>('span#fps')
		fpsEl.innerText = (1000 / duration).toFixed(1)
	}
}

const startAnimation = () => {
	// Reset the interval

	clearTimeout(animationIntervalID)

	// Check if we're on low memory mode

	if (lowMemMode) {
		renderTickLowMem(0)
		return
	}

	// Reset the graph

	createGraph()

	// Render the ticks

	renderTick(0)
}

// When page loads, show the run

addEventListener('load', async () => {
	document.querySelector<HTMLSpanElement>('#memory-mode-indicator').innerText =
		lowMemMode ? 'Low Memory Mode' : 'Regular Memory Mode'
	document.querySelector<HTMLButtonElement>('#memory-mode-button').innerText =
		lowMemMode ? 'Switch to Regular Memory Mode' : 'Switch to Low Memory Mode'
	const output = await getRunOutput(searchParams.get('id'))
	data = lowMemMode ? parseContaFileLowMem(output) : parseContaFile(output)

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

// Toggle low memory mode

const toggleMemoryMode = () => {
	if (lowMemMode) {
		searchParams.delete('low-memory')
	} else {
		searchParams.set('low-memory', '1')
	}

	location.search = '?' + searchParams.toString()
}