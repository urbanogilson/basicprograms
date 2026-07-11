# Release Cerrado locally on minikube

The `deploy/` Helm chart targets AKS (ACR images, Azure managed PostgreSQL via an
external Secret, ingress-nginx + TLS). To run the same chart locally we swap in:

- **Local images** loaded into minikube (no registry) — `deploy/values.local.yaml` sets `global.image.registry: ""`.
- **In-cluster Postgres** as the DB — `postgres.yaml` here (dev-only, throwaway `emptyDir`), plus the `cerrado-db` Secret the chart expects.
- **Ingress without TLS** on `cerrado.test`.

The chart is unchanged except that the image helpers now tolerate an empty registry.

## Prerequisites
`minikube` (v1.38+), `kubectl`, `helm`, `docker`.

## Steps (run from `k8s/cerrado`)

```bash
# 1. Cluster + ingress controller
minikube start
minikube addons enable ingress

# 2. Build images and load them into minikube (driver-agnostic)
docker build -f apps/cerrado-api/Dockerfile              -t cerrado-api:local     .
docker build -f apps/cerrado-api/Dockerfile --target builder -t cerrado-migrate:local .
docker build -f apps/cerrado-web/Dockerfile              -t cerrado-web:local     .
minikube image load cerrado-api:local cerrado-migrate:local cerrado-web:local

# 3. Namespace + local Postgres and the cerrado-db Secret (must exist before helm)
kubectl create namespace cerrado
kubectl apply -n cerrado -f minikube/postgres.yaml
kubectl -n cerrado rollout status deploy/postgres

# 4. Release the chart
helm install cerrado deploy -n cerrado -f deploy/values.local.yaml

# 5. Watch it come up
kubectl -n cerrado get pods
kubectl -n cerrado logs job/cerrado-migrate     # migration applied + "Seeded 4 agents."
```

## Access

The ingress host is `cerrado.test`. With minikube's **ingress-dns** addon the
`.test` TLD resolves to the minikube IP automatically — no `/etc/hosts` editing.

```bash
minikube addons enable ingress-dns   # once

# Point the host OS at minikube's DNS for the .test TLD (one-time, per OS):
# https://minikube.sigs.k8s.io/docs/handbook/addons/ingress-dns/
# Linux (systemd-resolved) example:
#   sudo mkdir -p /etc/systemd/resolved.conf.d
#   printf '[Resolve]\nDNS=%s\nDomains=~test\n' "$(minikube ip)" \
#     | sudo tee /etc/systemd/resolved.conf.d/minikube.conf
#   sudo systemctl restart systemd-resolved

# Then:
curl -s http://cerrado.test/api/agents      # or open http://cerrado.test
nslookup cerrado.test "$(minikube ip)"       # sanity-check resolution
```

Fallback without ingress/DNS:

```bash
kubectl -n cerrado port-forward svc/cerrado-web 8080:80
curl -s localhost:8080/api/agents
```

## Upgrade after code changes

```bash
# rebuild + reload the changed image(s), then:
helm upgrade cerrado deploy -n cerrado -f deploy/values.local.yaml
kubectl -n cerrado rollout restart deploy/cerrado-api deploy/cerrado-web
```
> `pullPolicy: IfNotPresent` + the same `:local` tag means you must reload the
> image and restart the deployment to pick up changes.

## Teardown

```bash
helm uninstall cerrado -n cerrado
kubectl delete ns cerrado
minikube stop            # or: minikube delete
```
