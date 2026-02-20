import {
  createContext,
  useCallback,
  useContext,
  useEffect,
  useState,
  type ReactNode,
} from 'react'
import type { User } from '@/lib/api'
import * as api from '@/lib/api'

/** Decode JWT payload (handles base64url from jsonwebtoken). */
function parseJwtPayload(token: string): { userId?: string; email?: string } | null {
  try {
    const part = token.split('.')[1]
    if (!part) return null
    const base64 = part.replace(/-/g, '+').replace(/_/g, '/')
    const padded = base64 + '='.repeat((4 - (base64.length % 4)) % 4)
    return JSON.parse(atob(padded))
  } catch {
    return null
  }
}

interface AuthContextValue {
  user: User | null
  loading: boolean
  signIn: (email: string, password: string) => Promise<{ data: { user: User; token: string } | null; error: Error | null }>
  signUp: (email: string, password: string) => Promise<{ data: { user: User; token: string } | null; error: Error | null }>
  signOut: () => void
}

const AuthContext = createContext<AuthContextValue | null>(null)

export function AuthProvider({ children }: { children: ReactNode }) {
  const [user, setUser] = useState<User | null>(null)
  const [loading, setLoading] = useState(true)

  const loadUser = useCallback(() => {
    const token = localStorage.getItem('token')
    if (!token) {
      setUser(null)
      setLoading(false)
      return
    }
    const payload = parseJwtPayload(token)
    if (payload?.userId) {
      setUser({ id: payload.userId, email: payload.email ?? '' })
    } else {
      api.clearToken()
      setUser(null)
    }
    setLoading(false)
  }, [])

  useEffect(() => {
    loadUser()
  }, [loadUser])

  const signUp = useCallback(async (email: string, password: string) => {
    try {
      const data = await api.register(email, password)
      api.setToken(data.token)
      setUser(data.user)
      return { data, error: null }
    } catch (err) {
      return { data: null, error: err as Error }
    }
  }, [])

  const signIn = useCallback(async (email: string, password: string) => {
    try {
      const data = await api.login(email, password)
      api.setToken(data.token)
      setUser(data.user)
      return { data, error: null }
    } catch (err) {
      return { data: null, error: err as Error }
    }
  }, [])

  const signOut = useCallback(() => {
    api.clearToken()
    setUser(null)
  }, [])

  const value: AuthContextValue = { user, loading, signIn, signUp, signOut }

  return <AuthContext.Provider value={value}>{children}</AuthContext.Provider>
}

export function useAuth(): AuthContextValue {
  const ctx = useContext(AuthContext)
  if (!ctx) throw new Error('useAuth must be used within AuthProvider')
  return ctx
}
