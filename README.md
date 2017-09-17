## CliWorkers
<b>CliWorkers</b> is a sample application which allows you to run several jobs in parallel.

### Build
The build was tested on windows platform, although it should be easy to port the application to unix.
##### Prerequisites:
1. Cmake
2. Qt 5.x

### Run
To run the application, call the executable with following arguments:

| Argument  | Description  |
| ------ | --- |
| -?, -h, --help   |  print help and exit |
| --threads \<N\>  |  run application with N workers, each worker will be executed in a separate thread | 

Inside the Cli, following commands are supported:

| Command  | Description  |
| ------ | --- |
| status  |  print status for each worker |
| help    | print help |
| pause \<id> |      pause worker with number \<id> |
| resume \<id> |     resume worker with number \<id> |
| stop \<id>    |    stop worker with number \<id> |
| command \<id> \<command> |  request worker number \<id> to execute \<command> |
| commands \<id>  |  list worker-specfic commands for worker number \<id> |
| quit   |  exit program |

### Extensions

Currently, only two workers are implemented: QuotesWorker, which spawns random quote once in a while, 
and FractalWorker, which draws Barnsley fern iteratively until and stores intermediate  results in the fractals/ directory 
in the directory where executable was started. 

To implement new worker, subclass Worker abstract class and implement:

* ```void run()``` method which is the entry point for the worker logic implementation
* ```QString getName() ``` method which is used by Cli to append names to the messages from workers when the latter arrive  

When implementing Worker, be aware of two things:

1. You should check for ```stopped``` field value of the Worker once in a while since this is the 
way Cli actually tries to stop worker when requested. 
2. Call ```onFinish()``` method (from the worker implementation code) when the worker has done its job

After you have implemented new worker, you can add it and probably alter the way workers are created in Cli.cpp, passing ```QVector``` of workers you want to run into ```WorkerManager```'s constructor.