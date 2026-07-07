import { PrismaClient } from '@prisma/client';

const prisma = new PrismaClient();

const agents = [
  {
    name: 'Explorer',
    description: 'Sweeps the codebase to locate relevant files and patterns.',
  },
  {
    name: 'Planner',
    description: 'Designs step-by-step implementation strategies for a task.',
  },
  {
    name: 'Reviewer',
    description: 'Inspects diffs for correctness bugs and simplifications.',
  },
  {
    name: 'Builder',
    description: 'Launches and drives the app to verify changes end-to-end.',
  },
];

async function main() {
  for (const agent of agents) {
    await prisma.agent.upsert({
      where: { name: agent.name },
      update: { description: agent.description },
      create: agent,
    });
  }
  console.log(`Seeded ${agents.length} agents.`);
}

main()
  .catch((error) => {
    console.error(error);
    process.exit(1);
  })
  .finally(async () => {
    await prisma.$disconnect();
  });
