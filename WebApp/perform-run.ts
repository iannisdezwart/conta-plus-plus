interface RunSettings {
	populationSize: string
	numberOfCommunities: string
	humanMaxVelocity: string
	humanSpreadProbability: string
	humanSpreadRange: string
	humanInfectionDuration: string
}

const performRun = () => {
	// Collect all settings

	const get = (q: string) => document.querySelector<HTMLInputElement>(q).value

	const settings: RunSettings = {
		populationSize: get('#population-size'),
		numberOfCommunities: get('#number-of-communities'),
		humanMaxVelocity: get('#human-max-velocity'),
		humanSpreadProbability: get('#human-spread-probability'),
		humanSpreadRange: get('#human-spread-range'),
		humanInfectionDuration: get('#human-infection-duration')
	}

	const req = new XMLHttpRequest()
	req.open('POST', '/run')

	req.addEventListener('readystatechange', () => {
		console.log(req.response)
	})

	req.send(JSON.stringify(settings))
}