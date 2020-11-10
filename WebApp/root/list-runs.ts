interface Run {
	id: string,
	dateCreated: number
}

const listRuns = () => new Promise<Run[]>((resolve, reject) => {	
	const req = new XMLHttpRequest()

	req.open('POST', '/list-runs')

	req.addEventListener('load', () => {
		resolve(JSON.parse(req.responseText) as Run[])
	})

	req.addEventListener('error', reject)

	req.send()
})

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
		const dateString =
			`${ date.getFullYear() }-${ date.getMonth() }-${ date.getDate() + 1 }`

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