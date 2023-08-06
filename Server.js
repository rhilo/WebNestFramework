const express = require('express');
const bodyParser = require('body-parser');
const connectDB = require('./config/Database');

// Create Express app
const app = express();

// Connect to Database
connectDB();

// Use body-parser middleware to parse JSON data
app.use(bodyParser.json());

// Define port
const port = process.env.PORT || 6000;

// Start server
app.listen(port, () => console.log(`Server running on port ${port}`));
