const performRun = () => {
	// Collect all settings

	const get = (q: string) => document.querySelector<HTMLInputElement>(q).value

	const req = new XMLHttpRequest()
	req.open('POST', '/run')

	req.setRequestHeader('population-size', get('#population-size'))
	req.setRequestHeader('number-of-communities', get('#number-of-communities'))
	req.setRequestHeader('human-max-velocity', get('#human-max-velocity'))
	req.setRequestHeader('human-spread-probability', get('#human-spread-probability'))
	req.setRequestHeader('human-spread-range', get('#human-spread-range'))
	req.setRequestHeader('human-infection-duration', get('#human-infection-duration'))

	const writeToConsole = (
		text: string,
		overwrite: boolean = false
	) => {
		const console = document.querySelector('#console')

		// Write text

		if (overwrite) {
			console.innerHTML = text
		} else {
			console.innerHTML += text
		}

		// Scroll to bottom

		console.scrollTo(0, console.scrollHeight)
	}

	const printResponseToConsole = () => {
		const output = (req.response as string).replace(/\n/g, '<br>')
		writeToConsole(output, true)
	}

	req.addEventListener('readystatechange', () => {
		if (req.readyState == 2) {
			// Headers received, append the live run details to the page

			document.querySelector<HTMLDivElement>('.live-run').style.display = 'block'
			document.querySelector('#run-id').innerHTML = req.getResponseHeader('run-id')
		} else if (req.readyState == 3) {
			// Body is being sent

			printResponseToConsole()
		} else if (req.readyState == 4) {
			// Body has been fully received

			printResponseToConsole()
			writeToConsole('<<< Run Finished!')
		}
	})

	req.send()
}
