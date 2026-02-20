import 'dotenv/config'
import pg from 'pg'

const connectionString = process.env.DATABASE_URL ?? 'postgres://postgres:postgres@localhost:5433/jpaint'
export const pool = new pg.Pool({ connectionString })
