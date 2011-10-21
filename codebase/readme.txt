
- Modify the "CODEROOT" variable in makefile.inc to point to the root
  of your code base

- Copy your own implementation of IX component to folder "ix", PF component to folder "pf".

- We added an Attribute struct and one funtion signature getAttributes() to rm.h. Please
  copy them to your rm.h, and implement getAttributes() method.

- Implement the query engine (QE):

   Go to folder "qe" and type in:

    make clean
    make
    ./qetest

   The program should work.  But it does nothing.  You are supposed to
   implement the API of the record manager defined in rm.h

- You cannot change those functions of the QE class defined in qe/qe.h.
  If you think some changes are really necessary, please contact us
  first.
