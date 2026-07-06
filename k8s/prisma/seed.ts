import 'dotenv/config';
import { Pool } from 'pg';
import { PrismaPg } from '@prisma/adapter-pg';
import { PrismaClient } from '@prisma/client';

const pool = new Pool({ connectionString: process.env.DATABASE_URL });
const adapter = new PrismaPg(pool);
const prisma = new PrismaClient({ adapter });

async function main() {
  await prisma.agent.createMany({
    data: [
      { name: 'Alpha', description: 'First agent — handles data ingestion and preprocessing.' },
      { name: 'Beta', description: 'Second agent — responsible for model inference and scoring.' },
      { name: 'Gamma', description: 'Third agent — manages reporting and alerting pipelines.' },
    ],
  });
  console.log('Seed complete.');
}

main().finally(() => prisma.$disconnect());
