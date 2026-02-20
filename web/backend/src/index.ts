import 'dotenv/config'
import express from 'express'
import cors from 'cors'
import { authRouter } from './routes/auth.js'
import { projectsRouter } from './routes/projects.js'

const app = express()
const port = Number(process.env.PORT) || 3001

app.use(cors({ origin: true, credentials: true }))
app.use(express.json({ limit: '10mb' }))

app.use('/auth', authRouter)
app.use('/projects', projectsRouter)

app.get('/health', (_req, res) => res.json({ ok: true }))

app.listen(port, () => {
  console.log(`API running at http://localhost:${port}`)
})
