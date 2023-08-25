

async function HealthCheck(){

    $('#app-img-switch').html('<img src="/static/img/super_cat_hunt.jpg" class="rounded-circle">')


    var resp = await axios.get('/healthcheck')

    
    var resp_data = resp.data

    var result_html  = `
    <thead>
      <tr>
        <th> Health </th>
      </tr>
    </thead>
    <tbody id="app-result-render-body">
    </tbody>
    `


    $('#app-result-render').html(result_html)


    var body_html = `
    <tr>
        <td> ${resp_data["health"]} </td>
    </tr>
    
    `
    $('#app-result-render-body').append(body_html)


        
    $('#app-img-switch').html('<img src="/static/img/super_cat.jpg" class="rounded-circle">')
}

