import { Component } from '@angular/core';
import { AgentsComponent } from './agents/agents.component';

@Component({
  imports: [AgentsComponent],
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.css',
})
export class App {}
