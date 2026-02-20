import { Router, Request, Response } from 'express'
import { authMiddleware } from '../auth.js'
import { pool } from '../db/pool.js'

export const projectsRouter = Router()
projectsRouter.use(authMiddleware)

projectsRouter.get('/', async (req: Request, res: Response) => {
  const user = (req as Request & { user: { id: string } }).user
  const { rows } = await pool.query(
    'SELECT id, name, created_at, updated_at FROM projects WHERE user_id = $1 ORDER BY updated_at DESC',
    [user.id]
  )
  res.json(rows)
})

projectsRouter.get('/:id', async (req: Request, res: Response) => {
  const user = (req as Request & { user: { id: string } }).user
  const { id } = req.params
  const { rows } = await pool.query(
    'SELECT id, name, canvas_data, created_at, updated_at FROM projects WHERE id = $1 AND user_id = $2',
    [id, user.id]
  )
  if (rows.length === 0) {
    res.status(404).json({ error: 'Project not found' })
    return
  }
  res.json(rows[0])
})

projectsRouter.post('/', async (req: Request, res: Response) => {
  const user = (req as Request & { user: { id: string } }).user
  const { name, canvas_data } = req.body as { name?: string; canvas_data?: unknown }
  const projectName = typeof name === 'string' && name.trim() ? name.trim() : 'Untitled'
  const { rows } = await pool.query(
    `INSERT INTO projects (user_id, name, canvas_data, updated_at)
     VALUES ($1, $2, $3, now())
     RETURNING id, name, canvas_data, created_at, updated_at`,
    [user.id, projectName, canvas_data ?? null]
  )
  res.status(201).json(rows[0])
})

projectsRouter.patch('/:id', async (req: Request, res: Response) => {
  const user = (req as Request & { user: { id: string } }).user
  const { id } = req.params
  const { name, canvas_data } = req.body as { name?: string; canvas_data?: unknown }
  const updates: string[] = []
  const values: unknown[] = []
  let n = 1
  if (typeof name === 'string' && name.trim()) {
    updates.push(`name = $${n++}`)
    values.push(name.trim())
  }
  if (canvas_data !== undefined) {
    updates.push(`canvas_data = $${n++}`)
    values.push(canvas_data)
  }
  if (updates.length === 0) {
    res.status(400).json({ error: 'Provide name and/or canvas_data' })
    return
  }
  updates.push('updated_at = now()')
  values.push(id, user.id)
  const { rows } = await pool.query(
    `UPDATE projects SET ${updates.join(', ')} WHERE id = $${n} AND user_id = $${n + 1} RETURNING id, name, canvas_data, created_at, updated_at`,
    values
  )
  if (rows.length === 0) {
    res.status(404).json({ error: 'Project not found' })
    return
  }
  res.json(rows[0])
})

projectsRouter.delete('/:id', async (req: Request, res: Response) => {
  const user = (req as Request & { user: { id: string } }).user
  const { id } = req.params
  const { rowCount } = await pool.query('DELETE FROM projects WHERE id = $1 AND user_id = $2', [id, user.id])
  if (rowCount === 0) {
    res.status(404).json({ error: 'Project not found' })
    return
  }
  res.status(204).send()
})
