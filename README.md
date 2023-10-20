LARPServer
==========

A simple server for Sci-Fi settings in LARP. I am not a web developer (you can judge this looking at the sources). I am a professionnal C++ programmer and this is the reason why I chose to write this in C++. It uses [CROW](https://github.com/ipkn/crow) and [sqlite](http://www.sqlite.org/).

You can use it for your setting at will! If you want to modify it or improve it just send me pull request and I will try to keep it up to date!

The front-end was written by my wife [alexxx](https://github.com/alexxxzzz) and she is not a web developer. If some front-end engineer want to change it please do!

## How to use

Under Unixes (Linux/OSX). It should be possible to run it under Windows but you would probably have to install something like Cygwin. I may try to produce binaries in the future.

```bash
> git clone https://github.com/anirul/LARPServer.git
> cd LARPServer
> mkdir build
> cmake ..
> make
> ./LARPServer -i ../web
```
Then you can open a web interface to it [localhost](http://localhost:8080) or change *localhost* to the host IP to access it from remote.

## How to customize for your setting

Basically just open the **data.json** and modify it.

```json
{
	"users" : [
		{
			"name"  : "test",
			"desc"  : "this is a test account",
			"pass"  : "test",
			"money" : 1000
		}
	]
}
```

After you have started the server it will generate **.db** files and these are the database files.

## Post mortem

I used it for a meurder party and it worked (mosly) there is still some bugs that should be solved but I don't really have time to investigate. In case you find anything or have any remarks just create issues.
