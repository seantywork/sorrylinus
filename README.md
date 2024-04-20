# sorrylinus

===========


I'm sorry Linus, for building the most useless software on earth to handle remotely

the most useful software on earth

**Disclaimer : I am not liable for any misuse of this code**
**Only use it on machines and in environments that you have explicit permissions and authrizations to do so**

# sorrylinus hub mode
## Public access point for controlling sorrylinus socket client

[feebdaed.xyz](https://feebdaed.xyz)

Now every component is written in C++, except for sorrylinus module\
which is in C

run option for public access is: 

```bash
    cd src && soliapp --sock --feeb
```

below is the resulting output of 'test-uname' flag 

tested on arm64

![result](docs/result-test-uname.png)

below is the raspberry pi 4 side ir transreceiver setting

![ir](docs/rpi-setting.jpg)

successful test has been recorded and published

[ir-success](https://youtube.com/shorts/hcPera-hTdc?feature=share)




License
-------

Refer to LICENSE file

Thanks To
-----------------
- Http Lib
[Yuji Hirose](https://github.com/yhirose/cpp-httplib)
- Ws Lib
[Peter Thorson](https://github.com/zaphoyd/websocketpp)
- JSON Parser
[Niels Lohmann](https://github.com/nlohmann/json)
- OAuth2
[Giuseppe Baccini](https://github.com/giubacc/cppOAuth2RestClient)
