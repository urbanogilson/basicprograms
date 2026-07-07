/**
 * This is not a production server yet!
 * This is only a minimal backend to get started.
 */

import express from 'express';
import * as path from 'path';
import { prisma } from './prisma';

const app = express();

app.use(express.json());
app.use('/assets', express.static(path.join(__dirname, 'assets')));

app.get('/api', (req, res) => {
  res.send({ message: 'Welcome to cerrado-api!' });
});

app.get('/api/health', async (req, res) => {
  try {
    await prisma.$queryRaw`SELECT 1`;
    res.send({ status: 'ok', db: 'up' });
  } catch (error) {
    console.error(error);
    res.status(503).send({ status: 'error', db: 'down' });
  }
});

app.get('/api/agents', async (req, res) => {
  try {
    const agents = await prisma.agent.findMany({
      orderBy: { createdAt: 'asc' },
    });
    res.send(agents);
  } catch (error) {
    console.error(error);
    res.status(500).send({ message: 'Failed to load agents' });
  }
});

const port = process.env.PORT || 3333;
const server = app.listen(port, () => {
  console.log(`Listening at http://localhost:${port}/api`);
});
server.on('error', console.error);
