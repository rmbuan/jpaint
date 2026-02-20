# jPaint Web (React + Postgres)

React frontend and Express + Postgres backend. Run everything locally with no cloud required.

## Run locally

### 1. Postgres

From the `web` folder:

```bash
docker compose up -d
```

(Or use a local Postgres install; create a database named `jpaint`.)

### 2. Backend

```bash
cd backend
cp .env.example .env
# Edit .env if needed (DATABASE_URL, JWT_SECRET)
npm install
npm run db:migrate
npm run dev
```

API runs at `http://localhost:3001`.

### 3. Frontend

From the `web` folder (not inside `backend`):

```bash
cp .env.example .env
# VITE_API_URL defaults to http://localhost:3001
npm install
npm run dev
```

App runs at `http://localhost:5173`. Log in or register; projects and canvas UI can be added next.

## Env summary

| Where     | Variable      | Example |
|----------|---------------|---------|
| backend  | `PORT`        | `3001` |
| backend  | `DATABASE_URL`| `postgres://postgres:postgres@localhost:5432/jpaint` |
| backend  | `JWT_SECRET`  | long random string |
| frontend | `VITE_API_URL`| `http://localhost:3001` |

## API

- `POST /auth/register` — body: `{ email, password }` → `{ user, token }`
- `POST /auth/login` — body: `{ email, password }` → `{ user, token }`
- `GET /projects` — header: `Authorization: Bearer <token>` → list projects
- `GET /projects/:id` — get one project (with `canvas_data`)
- `POST /projects` — body: `{ name?, canvas_data? }` → create
- `PATCH /projects/:id` — body: `{ name?, canvas_data? }` → update
- `DELETE /projects/:id` — delete
