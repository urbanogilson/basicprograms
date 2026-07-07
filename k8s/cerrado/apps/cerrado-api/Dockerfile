# Builder
FROM node:22-alpine AS builder
WORKDIR /app
COPY package.json package-lock.json ./
RUN npm ci
COPY . .
RUN npx prisma generate --schema apps/cerrado-api/prisma/schema.prisma \
 && npx nx build cerrado-api --configuration=production

# Runner
FROM node:22-alpine AS runner
ENV NODE_ENV=production
WORKDIR /app
RUN apk add --no-cache openssl
COPY --from=builder /app/dist/apps/cerrado-api/ ./
RUN npm ci --omit=dev
COPY --from=builder /app/node_modules/.prisma ./node_modules/.prisma
EXPOSE 3333
CMD ["node", "main.js"]
