import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';

export interface Agent {
  id: number;
  name: string;
  description: string;
  createdAt: string;
}

@Injectable({ providedIn: 'root' })
export class AgentService {
  private readonly apiUrl = 'http://localhost:3333/api/agents';

  constructor(private http: HttpClient) {}

  getAgents(): Observable<Agent[]> {
    return this.http.get<Agent[]>(this.apiUrl);
  }
}
