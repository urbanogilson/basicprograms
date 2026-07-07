import { PrismaClient } from '@prisma/client';

/**
 * Single shared PrismaClient instance. Reusing one client avoids exhausting
 * database connections when the dev server reloads on file changes.
 */
export const prisma = new PrismaClient();
