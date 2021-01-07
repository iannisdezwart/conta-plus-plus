interface Run {
	id: string,
	dateCreated: number
}

const listRuns = () => new Promise<Run[]>((resolve, reject) => {	
	const req = new XMLHttpRequest()

	req.open('POST', '/list-runs')

	req.addEventListener('load', () => {
		const runs: Run[] = req.responseText
			.substring(0, req.responseText.length - 1) // Remove trailing '\n'
			.split('\n') // Split into lines
			.map(el => {
				const split = el.split('\t')

				const run: Run = {
					id: split[0].replace('.conta', ''),
					dateCreated: parseInt(split[1]) * 1000
				}

				return run
			}) // Seperate file name and date

		resolve(runs)
	})

	req.addEventListener('error', reject)

	req.send()
})

const formatDateTime = (
	date: Date
) => {
	// Don't look at this monster please
	// All it does is return "YYYY-MM-DD hh:mm:ss"

	return `${ date.getFullYear() }-${ (date.getMonth() + 1).toString().padStart(2, '0') }-${ date.getDate().toString().padStart(2, '0') } ${ date.getHours().toString().padStart(2, '0') }:${ date.getMinutes().toString().padStart(2, '0') }:${ date.getSeconds().toString().padStart(2, '0') }`
}

addEventListener('load', async () => {
	const container = document.querySelector('.runs-container')
	const runs = await listRuns()

	let dynamicHTML = /* html */ `
	<table>
		<thead>
			<tr>
				<td>Run ID</td>
				<td>Date created</td>
				<td></td>
			</tr>
		</thead>
		<tbody>
	`

	for (let run of runs) {
		const date = new Date(run.dateCreated)
		const dateString = formatDateTime(date)

		dynamicHTML += /* html */ `
		<tr>
			<td>${ run.id }</td>
			<td>${ dateString }</td>
			<td>
				<a class="light button" href="/view-run?id=${ run.id }">view</a>
			</td>
		</tr>
		`
	}

	dynamicHTML += /* html */ `
		</tbody>
	</table>
	`

	container.innerHTML = dynamicHTML
})