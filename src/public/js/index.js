
async function getRecords(){

    var data = {"CMD":'none'}

    var gr = await axios.post('/get-records',data)

    var ret_data = gr.data

    alert(ret_data)

}