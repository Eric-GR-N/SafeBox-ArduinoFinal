//This is NodeJS - We have to install and import the software that we need

const path = require('path');
const express = require('express');
const mysql = require('mysql');
const app = express();
const ejs = require('ejs');
const bodyParser = require('body-parser');

app.use('/Public', express.static('Public')); //This command pushes the static files to the webpage, html/css etc

//Set view engine
app.set('view engine', 'ejs');
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

//Set views file
app.set('views',path.join(__dirname,'views'));



//Here we create the connection variable to the database
const db = mysql.createConnection({
    host: 'localhost',
    user: 'Eric',
    password: 'NoNoVember5-!',
    database: 'safebox'
});

//Here we make the actual connection to the database
db.connect(function(error){
    if(!!error)console.log(error);
    else console.log("Database Connected");
});



//request routes

app.get('/', function(request,response){

    let Sql = "SELECT * FROM data";
    db.query(Sql,(err,rows)=>{
        if(err)
        {
            throw err;
        }
        else
        {
            response.render('index',{table: rows});
        }
    })
});


app.get('/longest', function(request,response){

    let Sql = "SELECT MAX(AggregatedEndurance) AS max_AggregatedEndurance FROM endurance";
    db.query(Sql,(err,rows)=>{
        if(err)
        {
            throw err;
        }
        else
        {
            response.render('index',{table: rows});
        }
    })
});


app.get('/total', function(request,response){

    let Sql = "SELECT SUM(AggregatedEndurance) AS sum_AggregatedEndurance FROM endurance";
    db.query(Sql,(err,rows)=>{
        if(err)
        {
            throw err;
        }
        else
        {
            response.render('index',{table: rows});
        }
    })
});



app.listen(3000, function(){
    console.log("Hello, I'm listening!");
});

