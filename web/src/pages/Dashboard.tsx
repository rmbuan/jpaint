import { useEffect, useState } from 'react'
import { Link, useNavigate } from 'react-router-dom'
import { useAuth } from '@/contexts/AuthContext'
import * as api from '@/lib/api'

interface ProjectRow {
  id: string
  name: string
  created_at: string
  updated_at: string
}

function formatDate(iso: string) {
  const d = new Date(iso)
  return d.toLocaleDateString(undefined, { month: 'short', day: 'numeric', year: 'numeric' })
}

export function Dashboard() {
  const { user, signOut } = useAuth()
  const navigate = useNavigate()
  const [projects, setProjects] = useState<ProjectRow[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState('')
  const [creating, setCreating] = useState(false)
  const [newName, setNewName] = useState('')

  async function loadProjects() {
    setError('')
    try {
      const list = await api.fetchProjects()
      setProjects(list)
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Failed to load projects')
    } finally {
      setLoading(false)
    }
  }

  useEffect(() => {
    loadProjects()
  }, [])

  async function handleCreate(e: React.FormEvent) {
    e.preventDefault()
    if (!newName.trim()) return
    setCreating(true)
    setError('')
    try {
      const project = await api.createProject(newName.trim())
      setNewName('')
      navigate(`/project/${project.id}`)
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Failed to create project')
    } finally {
      setCreating(false)
    }
  }

  async function handleDelete(id: string, name: string) {
    if (!window.confirm(`Delete "${name}"?`)) return
    setError('')
    try {
      await api.deleteProject(id)
      setProjects((prev) => prev.filter((p) => p.id !== id))
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Failed to delete')
    }
  }

  return (
    <div style={{ padding: 24, maxWidth: 640, margin: '0 auto' }}>
      <header style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: 24 }}>
        <h1 style={{ margin: 0, fontSize: 24 }}>jPaint</h1>
        <span>
          {user?.email}{' '}
          <button type="button" onClick={signOut} style={{ marginLeft: 8 }}>
            Sign out
          </button>
        </span>
      </header>

      <section style={{ marginBottom: 24 }}>
        <h2 style={{ fontSize: 18, marginBottom: 12 }}>New project</h2>
        <form onSubmit={handleCreate} style={{ display: 'flex', gap: 8, alignItems: 'center' }}>
          <input
            type="text"
            placeholder="Project name"
            value={newName}
            onChange={(e) => setNewName(e.target.value)}
            disabled={creating}
            style={{ flex: 1, padding: '8px 12px' }}
          />
          <button type="submit" disabled={creating || !newName.trim()}>
            {creating ? 'Creating…' : 'Create'}
          </button>
        </form>
      </section>

      <section>
        <h2 style={{ fontSize: 18, marginBottom: 12 }}>Your projects</h2>
        {error && (
          <div style={{ marginBottom: 12 }}>
            <p style={{ color: 'red' }}>{error}</p>
            {error.includes("Can't reach the server") && (
              <p style={{ fontSize: 13, color: '#666', marginTop: 4 }}>
                Start the backend: open a terminal in the project folder and run <code>cd web/backend && npm run dev</code>
              </p>
            )}
          </div>
        )}
        {loading ? (
          <p>Loading projects…</p>
        ) : projects.length === 0 ? (
          <p style={{ color: '#666' }}>No projects yet. Create one above.</p>
        ) : (
          <ul style={{ listStyle: 'none', padding: 0, margin: 0 }}>
            {projects.map((p) => (
              <li
                key={p.id}
                style={{
                  display: 'flex',
                  alignItems: 'center',
                  justifyContent: 'space-between',
                  padding: '12px 0',
                  borderBottom: '1px solid #eee',
                }}
              >
                <div>
                  <Link to={`/project/${p.id}`} style={{ fontWeight: 600, marginRight: 12 }}>
                    {p.name}
                  </Link>
                  <span style={{ fontSize: 13, color: '#666' }}>Updated {formatDate(p.updated_at)}</span>
                </div>
                <span>
                  <Link to={`/project/${p.id}`} style={{ marginRight: 8 }}>
                    Open
                  </Link>
                  <button type="button" onClick={() => handleDelete(p.id, p.name)} style={{ color: '#c00' }}>
                    Delete
                  </button>
                </span>
              </li>
            ))}
          </ul>
        )}
      </section>
    </div>
  )
}
