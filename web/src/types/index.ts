export type ToolId =
  | 'line'
  | 'rectangle'
  | 'rectangleHollow'
  | 'ellipse'
  | 'ellipseHollow'
  | 'freehand'
  | 'fill'
  | 'text'
  | 'eraser'

export interface Point {
  x: number
  y: number
}

export interface DrawAction {
  tool: ToolId
  strokeStyle: string
  lineWidth: number
  fillStyle?: string
  points: Point[]
  // for shapes: start + end (or radius)
  start?: Point
  end?: Point
}

export interface Project {
  id: string
  user_id: string
  name: string
  thumbnail_url: string | null
  created_at: string
  updated_at: string
}

export interface ProjectWithData extends Project {
  canvas_data: string | null // JSON of DrawAction[]
}
