import { useEffect, useState, useCallback } from 'react'
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

export function useAuth() {
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

  const signUp = async (email: string, password: string) => {
    try {
      const data = await api.register(email, password)
      api.setToken(data.token)
      setUser(data.user)
      return { data, error: null }
    } catch (err) {
      return { data: null, error: err as Error }
    }
  }

  const signIn = async (email: string, password: string) => {
    try {
      const data = await api.login(email, password)
      api.setToken(data.token)
      setUser(data.user)
      return { data, error: null }
    } catch (err) {
      return { data: null, error: err as Error }
    }
  }

  const signOut = useCallback(() => {
    api.clearToken()
    setUser(null)
  }, [])

  return { user, loading, signUp, signIn, signOut }
}
