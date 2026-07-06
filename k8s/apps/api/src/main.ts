import 'dotenv/config';
import express from 'express';
import * as path from 'path';
import { Pool } from 'pg';
import { PrismaPg } from '@prisma/adapter-pg';
import { PrismaClient } from '@prisma/client';

const pool = new Pool({ connectionString: process.env.DATABASE_URL });
const adapter = new PrismaPg(pool);
const prisma = new PrismaClient({ adapter });

const app = express();

app.use('/assets', express.static(path.join(__dirname, 'assets')));
app.use(express.json());

app.use((_req, res, next) => {
  res.setHeader('Access-Control-Allow-Origin', '*');
  res.setHeader('Access-Control-Allow-Headers', 'Content-Type');
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  next();
});

app.get('/api', (_req, res) => {
  res.send({ message: 'Welcome to api!' });
});

app.get('/api/agents', async (_req, res) => {
  const agents = await prisma.agent.findMany({ orderBy: { createdAt: 'desc' } });
  res.json(agents);
});

app.post('/api/agents', async (req, res) => {
  const { name, description } = req.body;
  const agent = await prisma.agent.create({ data: { name, description } });
  res.status(201).json(agent);
});

const port = process.env.PORT || 3333;
const server = app.listen(port, () => {
  console.log(`Listening at http://localhost:${port}/api`);
});
server.on('error', console.error);
