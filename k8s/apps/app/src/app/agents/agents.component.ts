import { Component, OnInit } from '@angular/core';
import { CommonModule } from '@angular/common';
import { AgentService, Agent } from './agent.service';

@Component({
  selector: 'app-agents',
  standalone: true,
  imports: [CommonModule],
  templateUrl: './agents.component.html',
  styleUrl: './agents.component.css',
})
export class AgentsComponent implements OnInit {
  agents: Agent[] = [];

  constructor(private agentService: AgentService) {}

  ngOnInit(): void {
    this.agentService.getAgents().subscribe((data) => (this.agents = data));
  }
}
