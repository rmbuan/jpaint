const API_URL = import.meta.env.VITE_API_URL ?? 'http://localhost:3002'

function getToken(): string | null {
  return localStorage.getItem('token')
}

/** Run fetch; on network error throw a clear message so the user knows the backend may be down. */
async function apiFetch(url: string, options: RequestInit): Promise<Response> {
  try {
    return await fetch(url, options)
  } catch (e) {
    const msg = e instanceof Error ? e.message : 'Network error'
    throw new Error(`Can't reach the server at ${API_URL}. Is the backend running? (${msg})`)
  }
}

export interface User {
  id: string
  email: string
}

export interface AuthResponse {
  user: User
  token: string
}

async function authFetch(url: string, body: object): Promise<AuthResponse> {
  let res: Response
  try {
    res = await fetch(url, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(body),
    })
  } catch (e) {
    const msg = e instanceof Error ? e.message : 'Network error'
    throw new Error(`Cannot reach server at ${API_URL}. Is the backend running? (${msg})`)
  }
  const data = await res.json().catch(() => ({}))
  if (!res.ok) throw new Error((data as { error?: string }).error ?? `Request failed (${res.status})`)
  return data as AuthResponse
}

export async function register(email: string, password: string): Promise<AuthResponse> {
  return authFetch(`${API_URL}/auth/register`, { email, password })
}

export async function login(email: string, password: string): Promise<AuthResponse> {
  return authFetch(`${API_URL}/auth/login`, { email, password })
}

export function setToken(token: string) {
  localStorage.setItem('token', token)
}

export function clearToken() {
  localStorage.removeItem('token')
}

export async function fetchProjects(): Promise<{ id: string; name: string; created_at: string; updated_at: string }[]> {
  const token = getToken()
  if (!token) throw new Error('Not authenticated')
  const res = await apiFetch(`${API_URL}/projects`, {
    headers: { Authorization: `Bearer ${token}` },
  })
  if (!res.ok) {
    const data = await res.json().catch(() => ({}))
    throw new Error((data as { error?: string }).error ?? 'Failed to load projects')
  }
  return res.json()
}

export async function fetchProject(id: string): Promise<{
  id: string
  name: string
  canvas_data: unknown
  created_at: string
  updated_at: string
}> {
  const token = getToken()
  if (!token) throw new Error('Not authenticated')
  const res = await apiFetch(`${API_URL}/projects/${id}`, {
    headers: { Authorization: `Bearer ${token}` },
  })
  if (!res.ok) {
    const data = await res.json().catch(() => ({}))
    throw new Error((data as { error?: string }).error ?? 'Failed to load project')
  }
  return res.json()
}

export async function createProject(name: string, canvasData?: unknown) {
  const token = getToken()
  if (!token) throw new Error('Not authenticated')
  const res = await apiFetch(`${API_URL}/projects`, {
    method: 'POST',
    headers: { 'Content-Type': 'application/json', Authorization: `Bearer ${token}` },
    body: JSON.stringify({ name, canvas_data: canvasData ?? null }),
  })
  if (!res.ok) {
    const data = await res.json().catch(() => ({}))
    throw new Error((data as { error?: string }).error ?? 'Failed to create project')
  }
  return res.json()
}

export async function updateProject(id: string, payload: { name?: string; canvas_data?: unknown }) {
  const token = getToken()
  if (!token) throw new Error('Not authenticated')
  const res = await apiFetch(`${API_URL}/projects/${id}`, {
    method: 'PATCH',
    headers: { 'Content-Type': 'application/json', Authorization: `Bearer ${token}` },
    body: JSON.stringify(payload),
  })
  if (!res.ok) {
    const data = await res.json().catch(() => ({}))
    throw new Error((data as { error?: string }).error ?? 'Failed to update project')
  }
  return res.json()
}

export async function deleteProject(id: string) {
  const token = getToken()
  if (!token) throw new Error('Not authenticated')
  const res = await apiFetch(`${API_URL}/projects/${id}`, { method: 'DELETE', headers: { Authorization: `Bearer ${token}` } })
  if (!res.ok) {
    const data = await res.json().catch(() => ({}))
    throw new Error((data as { error?: string }).error ?? 'Failed to delete project')
  }
}
