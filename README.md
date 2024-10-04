# COP 4634 Project 2 - MT Collatz
## Authors: Noah Nickles, Dylan Stephens

### Purpose
This program processes a collatz sequence and uses multithreading to demonstrate the efficency and potential race conditions involved with it.

#### The program preforms the following operations: 
- Accepts a "-nolock" flag to prevent a mutex lock and expiriment with race conditions.
- Takes a value N which represents the maximum number range to compute.
- Take a value T which represents the number of threads to create.
- Outputs the stopping times to stdout and the elapsed time to stderr.
