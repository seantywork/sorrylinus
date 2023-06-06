
async function getRecords(){

    var data = {"CMD":'none'}

    var gr = await axios.post('/get-records',data)

    var ret_data = gr.data

    if(ret_data.length == 0){

        alert("Nothing to render")

        return
    }

    var keys = Object.keys(ret_data[0])


    var head = ''
    var body = ''

    for(let i=0;i<keys.length;i++){

        head += `<th>${keys[i]}</th>`


    }

    head = `<tr>${head}</tr>`

    
    for(let i=0;i<ret_data.length;i++){

        body += '<tr>'

        for(let j=0;j<keys.length;j++){

            body += `<td>${ret_data[i][keys[j]]}</td>`

        }


        body += '</tr>'
    }

    var inner_html = head + body

    $('#get-records-table').html(inner_html)
}