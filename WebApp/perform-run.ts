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

	req.addEventListener('readystatechange', () => {
		console.log(req.response)
	})

	req.send()
}
