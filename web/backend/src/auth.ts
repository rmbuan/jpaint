import type { Request, Response, NextFunction } from 'express'
import jwt from 'jsonwebtoken'
import { pool } from './db/pool.js'

const secret = process.env.JWT_SECRET ?? 'dev-secret-change-in-production'

export interface JwtPayload {
  userId: string
  email: string
}

export function signToken(payload: JwtPayload): string {
  return jwt.sign(payload, secret, { expiresIn: '7d' })
}

export function verifyToken(token: string): JwtPayload | null {
  try {
    const decoded = jwt.verify(token, secret) as JwtPayload
    return decoded
  } catch {
    return null
  }
}

export async function authMiddleware(req: Request, res: Response, next: NextFunction) {
  const auth = req.headers.authorization
  const token = auth?.startsWith('Bearer ') ? auth.slice(7) : null
  if (!token) {
    res.status(401).json({ error: 'Unauthorized' })
    return
  }
  const payload = verifyToken(token)
  if (!payload) {
    res.status(401).json({ error: 'Invalid or expired token' })
    return
  }
  const { rows } = await pool.query('SELECT id, email FROM users WHERE id = $1', [payload.userId])
  if (rows.length === 0) {
    res.status(401).json({ error: 'User not found' })
    return
  }
  ;(req as Request & { user: { id: string; email: string } }).user = { id: rows[0].id, email: rows[0].email }
  next()
}
