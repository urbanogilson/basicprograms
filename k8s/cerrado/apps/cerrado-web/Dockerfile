# Builder
FROM node:22-alpine AS builder
WORKDIR /app
COPY package.json package-lock.json ./
RUN npm ci
COPY . .
RUN npx nx build cerrado-web --configuration=production

# Runner
FROM nginx:1.27-alpine AS runner
COPY apps/cerrado-web/nginx.conf /etc/nginx/nginx.conf
COPY --from=builder /app/dist/apps/cerrado-web/browser /usr/share/nginx/html
EXPOSE 80
CMD ["nginx", "-g", "daemon off;"]
