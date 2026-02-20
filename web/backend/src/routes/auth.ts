import { Router, Request, Response } from 'express'
import bcrypt from 'bcryptjs'
import { pool } from '../db/pool.js'
import { signToken } from '../auth.js'

export const authRouter = Router()

authRouter.post('/register', async (req: Request, res: Response) => {
  try {
    const { email, password } = req.body as { email?: string; password?: string }
    if (!email || !password || typeof email !== 'string' || typeof password !== 'string') {
      res.status(400).json({ error: 'Email and password required' })
      return
    }
    if (password.length < 6) {
      res.status(400).json({ error: 'Password must be at least 6 characters' })
      return
    }
    const passwordHash = await bcrypt.hash(password, 10)
    const { rows } = await pool.query(
      'INSERT INTO users (email, password_hash) VALUES ($1, $2) RETURNING id, email',
      [email.trim().toLowerCase(), passwordHash]
    )
    const user = rows[0]
    const token = signToken({ userId: String(user.id), email: user.email })
    res.status(201).json({ user: { id: String(user.id), email: user.email }, token })
  } catch (err: unknown) {
    const e = err as { code?: string }
    if (e.code === '23505') {
      res.status(409).json({ error: 'Email already registered' })
      return
    }
    console.error('Register error:', err)
    res.status(500).json({ error: 'Registration failed. Try again.' })
  }
})

authRouter.post('/login', async (req: Request, res: Response) => {
  try {
    const { email, password } = req.body as { email?: string; password?: string }
    if (!email || !password || typeof email !== 'string' || typeof password !== 'string') {
      res.status(400).json({ error: 'Email and password required' })
      return
    }
    const emailNorm = email.trim().toLowerCase()
    const { rows } = await pool.query(
      'SELECT id, email, password_hash FROM users WHERE email = $1',
      [emailNorm]
    )
    if (rows.length === 0) {
      res.status(401).json({ error: 'Invalid email or password' })
      return
    }
    const user = rows[0]
    const ok = await bcrypt.compare(password, user.password_hash)
    if (!ok) {
      res.status(401).json({ error: 'Invalid email or password' })
      return
    }
    const token = signToken({ userId: String(user.id), email: user.email })
    res.json({ user: { id: String(user.id), email: user.email }, token })
  } catch (err) {
    console.error('Login error:', err)
    res.status(500).json({ error: 'Login failed. Try again.' })
  }
})
