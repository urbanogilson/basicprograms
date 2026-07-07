import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Observable } from 'rxjs';
import { environment } from '../../environments/environment';

export interface Agent {
  id: number;
  name: string;
  description: string;
  createdAt: string;
}

@Injectable({ providedIn: 'root' })
export class AgentService {
  private readonly apiUrl = `${environment.apiUrl}/agents`;

  constructor(private http: HttpClient) {}

  getAgents(): Observable<Agent[]> {
    return this.http.get<Agent[]>(this.apiUrl);
  }
}
