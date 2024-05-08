

var FRONT_WS = {}

var WS_READ_CHANNEL = {}

var REQUEST = {
    "access_token":"",
    "command": ""
}

var RESPONSE = {
    "status":"",
    "data":""
}

function FrontWSInit(){

    let or_url = window.location.href

    let proto_url = or_url.split('://') 

    let proto = proto_url[0]

    let urlbody_trail = proto_url[1].split('/')

    let urlbody = urlbody_trail[0]

    let trail_path = '/front-client'

    if (proto == 'http'){

        proto = 'ws://'

        let urlbody_port = urlbody.split(':')

        urlbody = urlbody_port[0] + ':' + '8000'

    } else if (proto == 'https'){

        proto = 'wss://'

        let urlbody_port = urlbody.split(':')

        urlbody = urlbody_port[0] 

    }

    let ws_url = proto + urlbody + trail_path

    console.log(ws_url)


    let ws = new WebSocket(ws_url)

    ws.onopen = function (){

        let req = JSON.parse(JSON.stringify(REQUEST))

        req.command = "REQ_KEY"
        req.access_token = getCookie("access_token")

        ws.send(JSON.stringify(req))

    }

    ws.onmessage = function (evnt){

        let resp = evnt.data

        WS_READ_CHANNEL = resp

    }

    
    return ws

}

async function CheckWSReadChannelWithTimeOut(limit){


    let counter = 0


    while (counter < limit){

        if (JSON.stringify(WS_READ_CHANNEL) == JSON.stringify({})){

            counter += 1 

        } else {
            console.log('read signal received')
            break
        }

        await Delay(1)

    }

    if (counter >= limit){

        alert('ws read time limit exceeded')

        return -1

    }

    return 0

}


async function RequestKey(){

    while(REQ_KEY == ""){

        WS_READ_CHANNEL = {}
        
        let req = JSON.parse(JSON.stringify(REQUEST))

        req.command = "REQ_KEY"
        req.access_token = getCookie("access_token")

        FRONT_WS.send(JSON.stringify(req))

        let ret_signal = await CheckWSReadChannelWithTimeOut(10000)

        if(ret_signal < 0){

            alert("get req key timed out")


        } else {

            
            var recv_data = JSON.parse(WS_READ_CHANNEL)

            if(recv_data.status != "FAIL"){
                console.log("got req key")
            
                REQ_KEY = recv_data.data

                return
            } else {
                console.log("failed to get req key")
            }
        }

    }
}

async function WSmain(){

    FRONT_WS = FrontWSInit()

    console.log('front web socket ready')

    await Delay(3000)
    
    await RequestKey()

}


WSmain()