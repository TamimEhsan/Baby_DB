# Adding very simple SQL skeleton
![](https://cstack.github.io/db_tutorial/assets/images/arch2.gif)


We will add some more functionalities to our existing database statements. We will support meta commands and normal statements separately. We do this for easy handle of the system. 

To handle meta commands and prepare statements we create two new structs. `Statement` `Statement_Type` for statements,  `MetaCommandResult` and `PrepareResult` for status passing. 

Initially they do not contain much but as we go we will populate more variables into them.  And currently we just identify statements as insert and select. We will process them later.

