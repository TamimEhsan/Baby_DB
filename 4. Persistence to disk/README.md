# Creating persistency

To create persistency we write the rows into files. To access file we create an abstruction called pager. Now the pages will be held by the pager structure.

At first we initialize the pager by opening a file descriptor and initializing each page as NULL as no data is fetched yet. 
Whenever we call to a page, we will first check if the page is in memory. If not, we will fetch it from file. 
Upon closing the database we will flush all the page to disk and free the pointers.

We use this pager now from table structure and everything remains same as before. So now even after closing and reopening the db we can see previous inserted data.

NOTE TO SELF:
To use constant across multiple files, declare constant int one file and assign values. When we want to use those constant in other file, use extern const to declare them as external constant. But do not assign values to them.