# Build
FROM node:22-alpine AS builder
WORKDIR /app
COPY package*.json ./
RUN npm ci
COPY prisma ./prisma
COPY prisma.config.ts ./
RUN npx prisma generate
COPY . .
RUN npx nx build api --configuration=production

# Runner
FROM node:22-alpine AS runner
WORKDIR /app
COPY --from=builder /app/dist/apps/api ./
RUN npm ci --omit=dev
COPY --from=builder /app/node_modules/@prisma/client ./node_modules/@prisma/client
COPY --from=builder /app/node_modules/.prisma ./node_modules/.prisma
EXPOSE 3333
CMD ["node", "main.js"]
