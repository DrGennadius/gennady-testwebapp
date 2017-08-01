var express = require('express');
var bodyParser = require('body-parser');
var app = express();
var addon = require('./app/build/Release/usercheck.node');
var port = 3000;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
	extended: true
})); 

app.get('/', function (req, res) {
	res.redirect('/usercheck');
});

app.get('/usercheck', function (req, res) {
	res.render('usercheck.ejs');
});

app.post('/usercheck', function (req, res) {
	if (addon.usercheck(req.body.username))
		res.render('success.ejs');
	else
		res.render('failure.ejs');
});

app.listen(port, function () {
	console.log('Web app listening on port '+port+'!');
});