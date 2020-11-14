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

	req.addEventListener('loadstart', () => {
		// Todo: this header does not get received, fix this

		console.log(req.getResponseHeader('run-id'))
	})

	let receivedBytes = 0

	req.addEventListener('readystatechange', () => {
		const response = req.response as string
		if (response.length == 0) return

		console.log(response.substring(receivedBytes))
		receivedBytes = response.length
	})

	req.addEventListener('load', () => {
		// console.log('Full response', req.response)
	})

	req.send()
}
