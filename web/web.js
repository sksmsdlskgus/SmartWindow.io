const express = require('express')
const path = require('path')
const { createProxyMiddleware } = require('http-proxy-middleware')

const app = express()
app.use('/web', express.static(path.join(process.cwd(), 'static')))

app.use('/api', createProxyMiddleware({
    target: 'http://172.20.10.7:80/',
    changeOrigin: true,
}))

app.listen(3000, () => {
    console.log('server opened')
})
