import * as http from 'http'
import * as fs from 'fs'

const runSimulator = (
	req: http.IncomingMessage,
	res: http.ServerResponse
) => {
	// Todo: make
}

// Sanitises a hex string

const sanitiseHex = (
	hexString: string
) => {
	hexString = hexString.toUpperCase()
	let output = ''

	// Loop over each character of the hexString input

	for (let char of hexString) {
		const charCode = char.charCodeAt(0)

		// If the char is [0-9] or [A-F], add it to the output

		if (charCode >= 0x30 && charCode <= 0x39 || charCode >= 0x41 && charCode <= 0x46) {
			output += char
		}
	}

	return output
}

// Function that sends a binary run output

const sendRunOutput = (
	req: http.IncomingMessage,
	res: http.ServerResponse
) => {
	const runID = sanitiseHex(req.headers['run-id'] as string)
	const filePath = `${ __dirname }/../Simulator/output/${ runID }.conta`

	if (!fs.existsSync(filePath)) {
		// The file does not exist, send 404

		res.statusCode = 404
		res.end('404: Not Found')
		return
	}

	// Set the mime type

	res.writeHead(200, {
		'Content-Type': 'application/conta'
	})

	// Stream the run output to the client

	const fileStream = fs.createReadStream(filePath)
	fileStream.pipe(res)
}

interface File {
	name: string
	mimeType: string
}

// Define the file routes

const files = new Map<string, File>([
	[ '/', { name: 'index.html', mimeType: 'text/html' } ],
	[ '/style', { name: 'style.css', mimeType: 'text/css' } ],
	[ '/file-buffer', { name: 'file-buffer.js', mimeType: 'text/javascript' } ],
	[ '/script', { name: 'script.js', mimeType: 'text/javascript' } ]
])

// Function that sends a defined file

const sendFile = (
	req: http.IncomingMessage,
	res: http.ServerResponse
) => {
	const file = files.get(req.url)

	if (file == null) {
		// The file does not exist, send 404

		res.statusCode = 404
		res.end('404: Not Found')
		return
	}

	// Send the file

	const filePath = `${ __dirname }/root/${ file.name }`

	if (!fs.existsSync(filePath)) {
		// File does not exist, but is defined in the files map

		res.statusCode = 500
		res.end('500: File is defined, but was not found')
		return
	}

	// Set the mime type

	res.writeHead(200, {
		'Content-Type': file.mimeType
	})

	// Stream the file to the client

	const fileStream = fs.createReadStream(filePath)
	fileStream.pipe(res)
}

// Create the server

const server = http.createServer((req, res) => {
	console.log(`[ Incoming Request ]: ${ req.url }`)

	if (req.method == 'POST') {
		// Handle POST requests

		if (req.url == '/run') {
			runSimulator(req, res)
		} else if (req.url == '/get-run-output') {
			sendRunOutput(req, res)
		} else {
			res.statusCode = 404
			res.end('404: Invalid Endpoint')
		}
	} else if (req.method == 'GET') {
		// Handle GET requests

		sendFile(req, res)
	} else {
		// The client used an invalid HTTP Method

		res.statusCode = 405
		res.end('405: Method Not Allowed')
	}
})

server.listen(1337)