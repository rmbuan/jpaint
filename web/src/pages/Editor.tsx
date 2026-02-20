import { useCallback, useEffect, useRef, useState } from 'react'
import { Link, useParams } from 'react-router-dom'
import { useAuth } from '@/contexts/AuthContext'
import * as api from '@/lib/api'

interface Point {
  x: number
  y: number
}

type ToolType = 'freehand' | 'line' | 'rect' | 'ellipse' | 'spray'
type StrokeKind = 'path' | 'line' | 'rect' | 'ellipse' | 'spray'

interface Stroke {
  type?: StrokeKind // default 'path' for backward compat
  points: Point[]
  color: string
  width: number
  sprayRadius?: number // for spray only
}

export function Editor() {
  const { id } = useParams<{ id: string }>()
  const canvasRef = useRef<HTMLCanvasElement>(null)
  const [projectName, setProjectName] = useState('')
  const [loading, setLoading] = useState(true)
  const [saving, setSaving] = useState(false)
  const [error, setError] = useState('')
  const [strokes, setStrokes] = useState<Stroke[]>([])
  const [currentStroke, setCurrentStroke] = useState<Point[]>([])
  const [color, setColor] = useState('#000000')
  const [lineWidth, setLineWidth] = useState(2)
  const [tool, setTool] = useState<ToolType>('freehand')
  const [spraySize, setSpraySize] = useState(24) // radius in px
  const isDrawing = useRef(false)
  const currentStrokeRef = useRef<Point[]>([])
  const colorRef = useRef(color)
  const lineWidthRef = useRef(lineWidth)
  const toolRef = useRef(tool)
  const spraySizeRef = useRef(spraySize)
  currentStrokeRef.current = currentStroke
  colorRef.current = color
  lineWidthRef.current = lineWidth
  toolRef.current = tool
  spraySizeRef.current = spraySize

  /** Commit the current stroke to the list. */
  const commitCurrentStroke = useCallback(() => {
    if (!isDrawing.current) {
      isDrawing.current = false
      return
    }
    const points = [...currentStrokeRef.current]
    const t = toolRef.current
    if (points.length === 0) {
      isDrawing.current = false
      return
    }
    const c = colorRef.current
    const w = lineWidthRef.current
    const sr = spraySizeRef.current
    isDrawing.current = false
    const kind: StrokeKind = t === 'freehand' ? 'path' : t === 'spray' ? 'spray' : t
    if (kind !== 'path' && kind !== 'spray' && points.length < 2) {
      isDrawing.current = false
      setCurrentStroke([])
      return
    }
    const stroke: Stroke =
      kind === 'spray'
        ? { type: 'spray', points, color: c, width: w, sprayRadius: sr }
        : { type: kind, points, color: c, width: w }
    setStrokes((prev) => [...prev, stroke])
    setCurrentStroke([])
  }, [])

  // Load project
  useEffect(() => {
    if (!id) return
    let cancelled = false
    setError('')
    api
      .fetchProject(id)
      .then((p) => {
        if (cancelled) return
        setProjectName(p.name)
        if (p.canvas_data && Array.isArray(p.canvas_data)) {
          setStrokes(p.canvas_data as Stroke[])
        } else {
          setStrokes([])
        }
      })
      .catch((e) => {
        if (!cancelled) setError(e instanceof Error ? e.message : 'Failed to load project')
      })
      .finally(() => {
        if (!cancelled) setLoading(false)
      })
    return () => {
      cancelled = true
    }
  }, [id])

  const drawStroke = useCallback((ctx: CanvasRenderingContext2D, stroke: Stroke) => {
    const type = stroke.type ?? 'path'
    ctx.strokeStyle = stroke.color
    ctx.fillStyle = stroke.color
    ctx.lineWidth = stroke.width
    ctx.lineCap = 'round'
    ctx.lineJoin = 'round'
    if (type === 'path') {
      if (stroke.points.length === 0) return
      if (stroke.points.length === 1) {
        ctx.beginPath()
        ctx.arc(stroke.points[0].x, stroke.points[0].y, stroke.width / 2, 0, Math.PI * 2)
        ctx.fill()
        return
      }
      ctx.beginPath()
      ctx.moveTo(stroke.points[0].x, stroke.points[0].y)
      for (let i = 1; i < stroke.points.length; i++) {
        ctx.lineTo(stroke.points[i].x, stroke.points[i].y)
      }
      ctx.stroke()
    } else if (type === 'line' && stroke.points.length >= 2) {
      ctx.beginPath()
      ctx.moveTo(stroke.points[0].x, stroke.points[0].y)
      ctx.lineTo(stroke.points[1].x, stroke.points[1].y)
      ctx.stroke()
    } else if (type === 'rect' && stroke.points.length >= 2) {
      const x0 = stroke.points[0].x
      const y0 = stroke.points[0].y
      const x1 = stroke.points[1].x
      const y1 = stroke.points[1].y
      const x = Math.min(x0, x1)
      const y = Math.min(y0, y1)
      const w = Math.abs(x1 - x0)
      const h = Math.abs(y1 - y0)
      ctx.strokeRect(x, y, w, h)
    } else if (type === 'ellipse' && stroke.points.length >= 2) {
      const x0 = stroke.points[0].x
      const y0 = stroke.points[0].y
      const x1 = stroke.points[1].x
      const y1 = stroke.points[1].y
      const cx = (x0 + x1) / 2
      const cy = (y0 + y1) / 2
      const rx = Math.abs(x1 - x0) / 2
      const ry = Math.abs(y1 - y0) / 2
      ctx.beginPath()
      ctx.ellipse(cx, cy, rx, ry, 0, 0, Math.PI * 2)
      ctx.stroke()
    } else if (type === 'spray' && stroke.points.length > 0) {
      const r = stroke.sprayRadius ?? 20
      const dotRadius = 1.5
      for (const p of stroke.points) {
        ctx.beginPath()
        ctx.arc(p.x, p.y, dotRadius, 0, Math.PI * 2)
        ctx.fill()
      }
    }
  }, [])

  const draw = useCallback(() => {
    const canvas = canvasRef.current
    if (!canvas) return
    const ctx = canvas.getContext('2d')
    if (!ctx) return
    const dpr = window.devicePixelRatio ?? 1
    const rect = canvas.getBoundingClientRect()
    canvas.width = rect.width * dpr
    canvas.height = rect.height * dpr
    ctx.scale(dpr, dpr)
    ctx.fillStyle = '#ffffff'
    ctx.fillRect(0, 0, rect.width, rect.height)
    for (const stroke of strokes) {
      drawStroke(ctx, stroke)
    }
    // Current stroke in progress
    if (currentStroke.length > 0) {
      const t = tool
      if (t === 'freehand' && currentStroke.length >= 2) {
        drawStroke(ctx, { type: 'path', points: currentStroke, color, width: lineWidth })
      } else if ((t === 'line' || t === 'rect' || t === 'ellipse') && currentStroke.length >= 2) {
        drawStroke(ctx, { type: t, points: [currentStroke[0], currentStroke[currentStroke.length - 1]], color, width: lineWidth })
      } else if (t === 'spray' && currentStroke.length > 0) {
        drawStroke(ctx, { type: 'spray', points: currentStroke, color, width: lineWidth, sprayRadius: spraySize })
      } else if (t === 'freehand' && currentStroke.length === 1) {
        ctx.fillStyle = color
        ctx.beginPath()
        ctx.arc(currentStroke[0].x, currentStroke[0].y, lineWidth / 2, 0, Math.PI * 2)
        ctx.fill()
      }
    }
  }, [strokes, currentStroke, color, lineWidth, tool, spraySize, drawStroke])

  useEffect(() => {
    draw()
  }, [draw])

  function getPoint(e: React.MouseEvent<HTMLCanvasElement>): Point {
    const canvas = canvasRef.current
    if (!canvas) return { x: 0, y: 0 }
    const rect = canvas.getBoundingClientRect()
    return {
      x: e.clientX - rect.left,
      y: e.clientY - rect.top,
    }
  }

  function handleMouseDown(e: React.MouseEvent<HTMLCanvasElement>) {
    isDrawing.current = true
    const pt = getPoint(e)
    if (tool === 'spray') {
      addSprayDots(pt, spraySize, 8)
    } else {
      setCurrentStroke([pt])
    }
  }

  function addSprayDots(center: Point, radius: number, count: number) {
    const dots: Point[] = []
    for (let i = 0; i < count; i++) {
      const angle = Math.random() * Math.PI * 2
      const r = Math.random() * radius
      dots.push({ x: center.x + Math.cos(angle) * r, y: center.y + Math.sin(angle) * r })
    }
    setCurrentStroke((prev) => [...prev, ...dots])
  }

  function handleMouseMove(e: React.MouseEvent<HTMLCanvasElement>) {
    if (!isDrawing.current) return
    const pt = getPoint(e)
    if (tool === 'freehand') {
      setCurrentStroke((prev) => [...prev, pt])
    } else if (tool === 'line' || tool === 'rect' || tool === 'ellipse') {
      setCurrentStroke((prev) => (prev.length > 0 ? [prev[0], pt] : [pt]))
    } else if (tool === 'spray') {
      addSprayDots(pt, spraySize, 5)
    }
  }

  function handleMouseUp() {
    commitCurrentStroke()
  }

  function handleMouseLeave() {
    commitCurrentStroke()
  }

  useEffect(() => {
    function onUp() {
      commitCurrentStroke()
    }
    window.addEventListener('mouseup', onUp)
    return () => window.removeEventListener('mouseup', onUp)
  }, [commitCurrentStroke])

  function handleUndo() {
    setStrokes((prev) => (prev.length === 0 ? prev : prev.slice(0, -1)))
  }

  async function handleSave() {
    if (!id) return
    setSaving(true)
    setError('')
    try {
      await api.updateProject(id, { canvas_data: strokes })
    } catch (e) {
      setError(e instanceof Error ? e.message : 'Failed to save')
    } finally {
      setSaving(false)
    }
  }

  if (!id) {
    return (
      <div style={{ padding: 24 }}>
        <p>Invalid project.</p>
        <Link to="/">Back to projects</Link>
      </div>
    )
  }

  if (loading) {
    return (
      <div style={{ padding: 24 }}>
        <p>Loading project…</p>
      </div>
    )
  }

  return (
    <div style={{ padding: 24 }}>
      <header style={{ display: 'flex', alignItems: 'center', gap: 16, marginBottom: 16, flexWrap: 'wrap' }}>
        <Link to="/">← Back</Link>
        <h1 style={{ margin: 0, fontSize: 20 }}>{projectName || 'Untitled'}</h1>
        <span style={{ display: 'flex', alignItems: 'center', gap: 6 }}>
          {(['freehand', 'line', 'rect', 'ellipse', 'spray'] as ToolType[]).map((t) => (
            <button
              key={t}
              type="button"
              onClick={() => setTool(t)}
              style={{
                padding: '6px 10px',
                fontWeight: tool === t ? 600 : 400,
                background: tool === t ? '#e0e0e0' : undefined,
              }}
              title={t === 'freehand' ? 'Free draw' : t === 'rect' ? 'Rectangle' : t === 'ellipse' ? 'Circle' : t === 'spray' ? 'Spray' : 'Line'}
            >
              {t === 'freehand' ? 'Free' : t === 'rect' ? '□' : t === 'ellipse' ? '○' : t === 'spray' ? 'Spray' : '—'}
            </button>
          ))}
        </span>
        <span style={{ marginLeft: 'auto', display: 'flex', alignItems: 'center', gap: 12 }}>
          <label style={{ display: 'flex', alignItems: 'center', gap: 4 }}>
            Color
            <input
              type="color"
              value={color}
              onChange={(e) => setColor(e.target.value)}
              style={{ width: 28, height: 28, padding: 0, border: '1px solid #ccc' }}
            />
          </label>
          {tool !== 'spray' && (
            <label style={{ display: 'flex', alignItems: 'center', gap: 4 }}>
              Size
              <input
                type="range"
                min={1}
                max={20}
                value={lineWidth}
                onChange={(e) => setLineWidth(Number(e.target.value))}
              />
              {lineWidth}
            </label>
          )}
          {tool === 'spray' && (
            <label style={{ display: 'flex', alignItems: 'center', gap: 4 }}>
              Spray size
              <input
                type="range"
                min={8}
                max={80}
                value={spraySize}
                onChange={(e) => setSpraySize(Number(e.target.value))}
              />
              {spraySize}
            </label>
          )}
          <button type="button" onClick={handleUndo} disabled={strokes.length === 0} title="Undo last stroke">
            Undo
          </button>
          <button type="button" onClick={handleSave} disabled={saving}>
            {saving ? 'Saving…' : 'Save'}
          </button>
        </span>
      </header>
      {error && <p style={{ color: 'red', marginBottom: 8 }}>{error}</p>}
      <div style={{ border: '1px solid #ccc', borderRadius: 4, overflow: 'hidden', background: '#fff' }}>
        <canvas
          ref={canvasRef}
          width={800}
          height={500}
          style={{ width: 800, height: 500, display: 'block', cursor: 'crosshair' }}
          onMouseDown={handleMouseDown}
          onMouseMove={handleMouseMove}
          onMouseUp={handleMouseUp}
          onMouseLeave={handleMouseLeave}
        />
      </div>
      <p style={{ fontSize: 13, color: '#666', marginTop: 8 }}>
        Use the tool buttons: Free draw, Line, Rectangle, Circle, Spray. Adjust color and size (or spray size), then Save.
      </p>
    </div>
  )
}
