# sorrylinus

I'm sorry Linus, for building the most useless software on earth to handle remotely

the most useful software on earth

**Disclaimer : I am not liable for any misuse of this code**
**Only use it on machines and in environments that you have explicit permissions and authrizations to do so**


[what on earth is this?](#what-on-earth-is-this)

[web](#web) 
- [sorrylinus-again](#sorrylinus-again)
- [hub](#hub) 
- [k8s](#k8s) 
- [nginx](#nginx) 
- [turn](#turn) 
- [db](#db) 
- [hack](#hack)



## what on earth is this?

This is a project that began with an aim to controlling an AC\
at my place from outside, but somehow has grown into something bigger (and weirder)

Below picture describes the whole structure

![structure](./doc/structure.drawio.png)


Below is the web dashboard, and you can find out more details about it \
further below at [sorrylinus-again](#sorrylinus-again)

[feebdaed.xyz](https://feebdaed.xyz)


Here is the video chat feature that is available under my permission



https://github.com/user-attachments/assets/b6ca19dc-2a87-422c-acd3-381d71b70e82



And this is CCTV feature using sorrylinus hub and sorrylinus 




https://github.com/user-attachments/assets/6856c1c1-a897-4265-ac4f-62ab333ec9c9



This repository is primarily about the sorrylinus C client program running on RPi, \
but the sweetest part IMO is the web part that enables the remote control.

See below.



## web


### sorrylinus-again

The Go project that powers the more maintainable web interface/dashboard\
for not only interacting with sorrylinus module but also\
for me having fun writing and chatting.

Here is the repository of [sorrylinus-again](https://github.com/seantywork/sorrylinus-again)


### hub

The C message hub that communicates with sorrylinus-again through Web Socket and\
with sorrylinus through TCP/TLS socket

### k8s

So grateful for numerous and endless features that k8s supports, here is the list

- 

### nginx

To conveniently and securely manage traffic going through feebdaed.xyz domain

### turn

Needed for WebRTC

### db 

Not yet integrated, as it's more than enough for this application to simply use\
file system and NFS

### hack

A bunch of shell scripts to set up and maintain this project, including cicd pipeline


