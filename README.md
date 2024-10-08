# COP 4634 Project 2 - MT Collatz
## Authors: Noah Nickles, Dylan Stephens

### Purpose
This program processes a collatz sequence and uses multithreading to demonstrate the efficency and potential race conditions involved with it.

#### The program preforms the following operations: 
- Takes a value N which represents the maximum number range to compute.
- Takes a value T which represents the number of threads to create.
- Accepts a "-nolock" flag to prevent a mutex lock and expiriment with race conditions.
- Outputs the stopping times to stdout and the elapsed time to stderr.

#### Encountered Problems:
- No problems were encountered when creating this program and with the data that was generated.
