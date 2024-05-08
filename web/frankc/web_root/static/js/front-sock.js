async function SoliQuery(sock_query_el){

    let target_el = '#' + sock_query_el

    let sock_query = $(target_el).val()

    if(!sock_query){
        alert('feed correct query')
        return
    }


    READ_CHANNEL = ""

    FRONT_WS.send(sock_query)

    let ret_signal = await CheckReadChannelWithTimeOut(10000)

    if (ret_signal == 1){
        return
    }


    RenderOutput(sock_query, READ_CHANNEL)


}