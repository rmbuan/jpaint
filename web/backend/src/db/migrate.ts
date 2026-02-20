import 'dotenv/config'
import pg from 'pg'
import { readFileSync } from 'fs'
import { fileURLToPath } from 'url'
import { dirname, join } from 'path'

const __dirname = dirname(fileURLToPath(import.meta.url))
const connectionString = process.env.DATABASE_URL ?? 'postgres://postgres:postgres@localhost:5432/jpaint'

async function migrate() {
  const client = new pg.Client({ connectionString })
  await client.connect()
  try {
    const sql = readFileSync(join(__dirname, 'schema.sql'), 'utf-8')
    await client.query(sql)
    console.log('Migration complete.')
  } finally {
    await client.end()
  }
}

migrate().catch((err) => {
  console.error(err)
  process.exit(1)
})
