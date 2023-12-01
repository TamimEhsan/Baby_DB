# Creating a basic in Memory DB
In this step we will create the database. But for now the DB will be

- Memory only, no disk persistency
- Only insert and print all rows
- Only a single table
- the table will have
    - id
    - username
    - email

Things will look something like this
```sqlite
db > select
Executed.
db > insert 1 tamim tamim@ehsan.comExecuted.
db > select
(1, tamim, tamim@ehsan.com)        
Executed.
db > insert 2 ehsan ehsan@tamim.comExecuted.
db > select
(1, tamim, tamim@ehsan.com)
(2, ehsan, ehsan@tamim.com)
Executed.
```



We save the rows in memory pages. We declare 100 pages of 4KB size. and each page will contain some rows. In this stage the rows will be saved as array. Later B-tree will be implemented.

To compile and run

```bash
 g++ main.c && ./a.out 
```

